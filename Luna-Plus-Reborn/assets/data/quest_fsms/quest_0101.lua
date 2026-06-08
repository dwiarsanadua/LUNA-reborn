--[[
  Quest 101: Hunting Goblin Archers
  Level Required: 17
  NPC Start: 20, NPC Complete: 20
  Rewards: EXP=4161, Gold=1200
]]

local fsm = require('fsm_engine')

local quest_101 = fsm:new({
    id = 101,
    name = "Hunting Goblin Archers",
    level_required = 17,
    npc_start = 20,
    npc_complete = 20,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 101: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 101: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 101: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 101: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 101: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 101: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 101: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 101: Rewards given")
                self:give_rewards({
                    exp = 4161,
                    gold = 1200,
                    items = {
                        { item_id = 21001505, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_101
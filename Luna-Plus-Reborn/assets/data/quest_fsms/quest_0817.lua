--[[
  Quest 817: Researching a Poison 4
  Level Required: 72
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [816]
  Rewards: EXP=1712847, Gold=34761
]]

local fsm = require('fsm_engine')

local quest_817 = fsm:new({
    id = 817,
    name = "Researching a Poison 4",
    level_required = 72,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {816},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 817: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 817: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 817: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 817: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 817: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 817: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 817: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 817: Rewards given")
                self:give_rewards({
                    exp = 1712847,
                    gold = 34761,
                    items = {
                        { item_id = 385, count = 25 },
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_817
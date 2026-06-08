--[[
  Quest 134: Hunting Goblin Patriarchs
  Level Required: 17
  NPC Start: 20, NPC Complete: 20
  Prerequisites: [133]
  Rewards: EXP=4161, Gold=2200
]]

local fsm = require('fsm_engine')

local quest_134 = fsm:new({
    id = 134,
    name = "Hunting Goblin Patriarchs",
    level_required = 17,
    npc_start = 20,
    npc_complete = 20,
    prerequisites = {133},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 134: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 134: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 134: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 134: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 134: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 134: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 134: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 134: Rewards given")
                self:give_rewards({
                    exp = 4161,
                    gold = 2200,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_134
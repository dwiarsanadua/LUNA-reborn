--[[
  Quest 108: Stolen Armor
  Level Required: 16
  NPC Start: 57, NPC Complete: 57
  Prerequisites: [105]
  Rewards: EXP=4683, Gold=1725
]]

local fsm = require('fsm_engine')

local quest_108 = fsm:new({
    id = 108,
    name = "Stolen Armor",
    level_required = 16,
    npc_start = 57,
    npc_complete = 57,
    prerequisites = {105},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 108: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 108: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 108: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 108: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 108: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 108: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 108: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 108: Rewards given")
                self:give_rewards({
                    exp = 4683,
                    gold = 1725,
                    items = {
                        { item_id = 48, count = 1 },
                        { item_id = 48, count = 10 },
                        { item_id = 21000012, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_108
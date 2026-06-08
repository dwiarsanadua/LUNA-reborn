--[[
  Quest 96: Lady's Greed
  Level Required: 16
  NPC Start: 18, NPC Complete: 18
  Rewards: EXP=4683, Gold=1725
]]

local fsm = require('fsm_engine')

local quest_96 = fsm:new({
    id = 96,
    name = "Lady's Greed",
    level_required = 16,
    npc_start = 18,
    npc_complete = 18,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 96: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 96: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 96: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 41,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 96: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 96: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 96: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 96: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 96: Rewards given")
                self:give_rewards({
                    exp = 4683,
                    gold = 1725,
                    items = {
                        { item_id = 39, count = 1 },
                        { item_id = 39, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_96
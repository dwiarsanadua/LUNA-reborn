--[[
  Quest 707: Farm Repair
  Level Required: 60
  NPC Start: 45, NPC Complete: 45
  Prerequisites: [706]
  Rewards: EXP=729940
]]

local fsm = require('fsm_engine')

local quest_707 = fsm:new({
    id = 707,
    name = "Farm Repair",
    level_required = 60,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {706},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 707: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 707: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 707: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 707: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 707: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 707: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 707: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 707: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 707: Rewards given")
                self:give_rewards({
                    exp = 729940,
                    items = {
                        { item_id = 353, count = 1 },
                        { item_id = 353, count = 1 },
                        { item_id = 354, count = 1 },
                        { item_id = 354, count = 1 },
                        { item_id = 353, count = 30 },
                        { item_id = 354, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_707
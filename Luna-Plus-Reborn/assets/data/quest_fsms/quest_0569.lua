--[[
  Quest 569: Quest 569
  Level Required: 51
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=500000
]]

local fsm = require('fsm_engine')

local quest_569 = fsm:new({
    id = 569,
    name = "Quest 569",
    level_required = 51,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 569: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 569: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 569: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 569: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 569: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 569: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 569: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 569: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 569: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 569: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 569: Rewards given")
                self:give_rewards({
                    exp = 500000,
                    items = {
                        { item_id = 21001049, count = 1 },
                        { item_id = 21001048, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_569
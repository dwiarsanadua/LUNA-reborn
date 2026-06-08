--[[
  Quest 565: Quest 565
  Level Required: 11
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=5000
]]

local fsm = require('fsm_engine')

local quest_565 = fsm:new({
    id = 565,
    name = "Quest 565",
    level_required = 11,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 565: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 565: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 565: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 8,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 565: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 565: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 565: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 565: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 565: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 565: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 565: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 565: Rewards given")
                self:give_rewards({
                    exp = 5000,
                    items = {
                        { item_id = 21001049, count = 1 },
                        { item_id = 21001048, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_565
--[[
  Quest 567: Quest 567
  Level Required: 31
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=70000
]]

local fsm = require('fsm_engine')

local quest_567 = fsm:new({
    id = 567,
    name = "Quest 567",
    level_required = 31,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 567: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 567: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 567: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 567: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 567: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 567: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 567: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 567: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 567: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 567: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 567: Rewards given")
                self:give_rewards({
                    exp = 70000,
                    items = {
                        { item_id = 21001049, count = 1 },
                        { item_id = 21001048, count = 1 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_567
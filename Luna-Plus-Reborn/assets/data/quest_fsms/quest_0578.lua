--[[
  Quest 578: Quest 578
  Level Required: 31
  NPC Start: 199, NPC Complete: 199
  Prerequisites: [567]
  Rewards: EXP=70000
]]

local fsm = require('fsm_engine')

local quest_578 = fsm:new({
    id = 578,
    name = "Quest 578",
    level_required = 31,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {567},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 578: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 578: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 578: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 578: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 578: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 578: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 578: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 578: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 578: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 578: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 578: Rewards given")
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

return quest_578
--[[
  Quest 580: Quest 580
  Level Required: 51
  NPC Start: 199, NPC Complete: 199
  Prerequisites: [569]
  Rewards: EXP=500000
]]

local fsm = require('fsm_engine')

local quest_580 = fsm:new({
    id = 580,
    name = "Quest 580",
    level_required = 51,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {569},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 580: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 580: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 580: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 580: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 580: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 580: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 580: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 580: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 580: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 580: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 580: Rewards given")
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

return quest_580
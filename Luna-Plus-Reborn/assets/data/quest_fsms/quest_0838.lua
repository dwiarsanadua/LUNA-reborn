--[[
  Quest 838: Certain Procedures
  Level Required: 84
  NPC Start: 128, NPC Complete: 128
  Prerequisites: [837]
  Rewards: EXP=2964862
]]

local fsm = require('fsm_engine')

local quest_838 = fsm:new({
    id = 838,
    name = "Certain Procedures",
    level_required = 84,
    npc_start = 128,
    npc_complete = 128,
    prerequisites = {837},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 838: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 838: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 838: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 838: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 838: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 838: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 838: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 838: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 838: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 838: Rewards given")
                self:give_rewards({
                    exp = 2964862,
                })
            end,
            transitions = {},
        },
    },
})

return quest_838
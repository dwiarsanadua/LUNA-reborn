--[[
  Quest 948: [Party] The Truth about George and Richard
  Level Required: 0
  NPC Start: 542, NPC Complete: 542
  Prerequisites: [947]
  Rewards: Gold=101632
]]

local fsm = require('fsm_engine')

local quest_948 = fsm:new({
    id = 948,
    name = "[Party] The Truth about George and Richard",
    level_required = 0,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {947},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 948: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 948: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 948: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 948: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 337,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 948: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 338,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 948: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 948: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 948: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 948: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 948: Rewards given")
                self:give_rewards({
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_948
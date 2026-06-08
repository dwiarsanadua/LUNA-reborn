--[[
  Quest 714: [Party]Attack Prevention
  Level Required: 0
  NPC Start: 45, NPC Complete: 45
  Prerequisites: [710]
  Rewards: EXP=617324
]]

local fsm = require('fsm_engine')

local quest_714 = fsm:new({
    id = 714,
    name = "[Party]Attack Prevention",
    level_required = 0,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {710},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 714: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 714: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 714: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 151,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 714: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 153,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 714: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 714: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 714: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 714: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 714: Rewards given")
                self:give_rewards({
                    exp = 617324,
                })
            end,
            transitions = {},
        },
    },
})

return quest_714
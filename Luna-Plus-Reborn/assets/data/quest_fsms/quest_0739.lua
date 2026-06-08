--[[
  Quest 739: [Quest] Request of a Butterfly
  Level Required: 61
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [738]
  Rewards: EXP=722601
]]

local fsm = require('fsm_engine')

local quest_739 = fsm:new({
    id = 739,
    name = "[Quest] Request of a Butterfly",
    level_required = 61,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {738},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 739: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 739: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 739: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 286,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 739: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 739: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 739: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 739: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 739: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 739: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 739: Rewards given")
                self:give_rewards({
                    exp = 722601,
                })
            end,
            transitions = {},
        },
    },
})

return quest_739
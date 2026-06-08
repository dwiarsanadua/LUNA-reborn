--[[
  Quest 427: [Party][Quest]The Third Qualifying Exam
  Level Required: 83
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [426]
  Rewards: EXP=2717141
]]

local fsm = require('fsm_engine')

local quest_427 = fsm:new({
    id = 427,
    name = "[Party][Quest]The Third Qualifying Exam",
    level_required = 83,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {426},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 427: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 427: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 427: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 427: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 427: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 427: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 427: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 427: Rewards given")
                self:give_rewards({
                    exp = 2717141,
                    items = {
                        { item_id = 30000060, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_427
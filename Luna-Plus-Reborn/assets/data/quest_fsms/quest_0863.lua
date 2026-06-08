--[[
  Quest 863: Everyone SHH!
  Level Required: 90
  NPC Start: 91, NPC Complete: 91
  Prerequisites: [862]
  Rewards: EXP=5222395, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_863 = fsm:new({
    id = 863,
    name = "Everyone SHH!",
    level_required = 90,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {862},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 863: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 863: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 863: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 709,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 863: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 863: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 863: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 863: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 863: Rewards given")
                self:give_rewards({
                    exp = 5222395,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_863
--[[
  Quest 426: [Party][Quest]The second Qualifying exam
  Level Required: 82
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [425]
  Rewards: EXP=2484438
]]

local fsm = require('fsm_engine')

local quest_426 = fsm:new({
    id = 426,
    name = "[Party][Quest]The second Qualifying exam",
    level_required = 82,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {425},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 426: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 426: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 426: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 426: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 426: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 426: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 426: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 426: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                    items = {
                        { item_id = 30000089, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_426